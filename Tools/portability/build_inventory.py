"""Inventory literal VS build declarations; never execute or evaluate MSBuild/PowerShell."""
import argparse
import json
from pathlib import Path
import re
import sys
import xml.etree.ElementTree as ET

NS = {'m': 'http://schemas.microsoft.com/developer/msbuild/2003'}


def local_path(owner, value, root):
    if any(token in value for token in ('$(', '%(', '*', '?')):
        raise ValueError(f'Non-literal project path in {owner.name}: {value}')
    path = (owner.parent / value.replace('\\', '/')).resolve()
    path.relative_to(root)  # Do not follow project references outside the repository.
    if not path.is_file():
        raise ValueError(f'Missing project or case mismatch: {path.relative_to(root)}')
    return path


def solution_projects(path, root):
    text = path.read_text(encoding='utf-8-sig')
    projects = {}
    dependencies = {}
    pattern = r'^Project\("[^"\n]+"\) = "[^"]+", "([^"]+)", "([^"]+)"\s*\n(.*?)^EndProject\s*$'
    for match in re.finditer(pattern, text, re.M | re.S):
        name, guid, body = match.groups()
        if not name.lower().endswith('.vcxproj'):
            continue
        projects[guid.upper()] = local_path(path, name, root)
        dependencies[guid.upper()] = re.findall(r'^\s*(\{[^}]+\})\s*=', body, re.M)
    if not projects:
        raise ValueError(f'No C++ projects found in {path.name}')
    edges = []
    for guid, required in dependencies.items():
        for dependency in required:
            if dependency.upper() not in projects:
                raise ValueError(f'Unknown solution dependency: {dependency}')
            edges.append([projects[guid], projects[dependency.upper()]])
    return list(projects.values()), edges


def project_details(path, root):
    tree = ET.parse(path).getroot()
    declarations = []
    references = []
    # Retain ancestor conditions: this is a union of declarations, not evaluated configurations.
    interesting = {'ClCompile', 'ProjectReference', 'AdditionalDependencies',
                   'AdditionalIncludeDirectories', 'AdditionalLibraryDirectories', 'Import',
                   'CustomBuild', 'Command', 'Outputs', 'ExcludedFromBuild', 'ConfigurationType'}

    def visit(node, conditions):
        conditions = conditions + ([node.attrib['Condition']] if 'Condition' in node.attrib else [])
        tag = node.tag.rsplit('}', 1)[-1]
        value = node.attrib.get('Include', node.attrib.get('Project', (node.text or '').strip()))
        if tag in interesting and value:
            declarations.append({'kind': tag, 'value': value, 'conditions': conditions})
        if tag == 'ProjectReference' and 'Include' in node.attrib:
            references.append(local_path(path, node.attrib['Include'], root))
        for child in node:
            visit(child, conditions)
    visit(tree, [])
    sources = sorted({n.attrib['Include'] for n in tree.findall('.//m:ClCompile', NS)
                      if 'Include' in n.attrib})
    return {'path': path.relative_to(root).as_posix(), 'source_count': len(sources),
            'sources': sources, 'declarations': declarations}, references


def inventory(root, entry_points):
    root = root.resolve()
    pending = []
    solution_edges = []
    for entry in entry_points:
        path = (root / entry).resolve()
        path.relative_to(root)
        if path.suffix == '.sln':
            projects, edges = solution_projects(path, root)
            pending.extend(projects)
            solution_edges.extend(edges)
        else:
            pending.append(path)
    projects = {}
    reference_edges = []
    while pending:
        path = pending.pop()
        if path in projects:
            continue
        detail, references = project_details(path, root)
        projects[path] = detail
        reference_edges.extend([path, target] for target in references)
        pending.extend(references)
    def edges_json(edges):
        return sorted({(a.relative_to(root).as_posix(), b.relative_to(root).as_posix())
                       for a, b in edges})
    return {'schema_version': 1,
            'scope': 'Literal declarations across all configurations; no MSBuild evaluation. '
                     'Imports, implicit library dependencies and custom commands are not expanded.',
            'projects': sorted(projects.values(), key=lambda item: item['path']),
            'solution_dependencies': edges_json(solution_edges),
            'project_references': edges_json(reference_edges)}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[2])
    args = parser.parse_args()
    try:
        root = args.root.resolve()
        wrapper = (root / 'Build-Conquest.ps1').read_text()
        block = re.search(r'\$engineProjects\s*=\s*@\((.*?)\n\)', wrapper, re.S)
        if not block:
            raise ValueError('Cannot identify engineProjects in Build-Conquest.ps1')
        engines = re.findall(r"Join-Path \$repoRoot '([^']+\.vcxproj)'", block[1])
        if not engines:
            raise ValueError('No literal engine project roots found')
        entries = ['Code/App/Src/Conquest.sln', 'Code/App/Src/Scripts/ConquestCampaign.sln']
        entries.extend(value.replace('\\', '/') for value in engines)
        result = inventory(root, entries)
        result['entry_points'] = entries
        print(json.dumps(result, indent=2, sort_keys=True))
    except (OSError, ValueError, ET.ParseError) as error:
        print(f'Build inventory failed: {error}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
