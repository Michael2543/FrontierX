"""Synthetic build-graph checks; no Windows tools or game data required."""
import tempfile
from pathlib import Path
import unittest

from build_inventory import inventory, solution_projects


class InventoryTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)

    def project(self, name, body=''):
        path = self.root / name
        path.write_text('<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">'
                        + body + '</Project>')
        return path

    def test_reference_cycle_and_conditions(self):
        self.project('a.vcxproj', '''<ItemDefinitionGroup><ProjectReference>
            <LinkLibraryDependencies>true</LinkLibraryDependencies></ProjectReference>
            </ItemDefinitionGroup><ItemGroup Condition="Debug">
            <ProjectReference Include="b.vcxproj"/><ClCompile Include="source.cpp">
            <ExcludedFromBuild Condition="Release">true</ExcludedFromBuild>
            </ClCompile></ItemGroup>''')
        self.project('b.vcxproj', '<ItemGroup><ProjectReference Include="a.vcxproj"/></ItemGroup>')
        result = inventory(self.root, ['a.vcxproj'])
        self.assertEqual(len(result['projects']), 2)
        self.assertEqual(len(result['project_references']), 2)
        excluded = next(d for d in result['projects'][0]['declarations']
                        if d['kind'] == 'ExcludedFromBuild')
        self.assertEqual(excluded['conditions'], ['Debug', 'Release'])
        self.assertEqual(result, inventory(self.root, ['a.vcxproj']))

    def test_missing_reference_fails(self):
        self.project('a.vcxproj', '<ItemGroup><ProjectReference Include="missing.vcxproj"/></ItemGroup>')
        with self.assertRaisesRegex(ValueError, 'Missing project'):
            inventory(self.root, ['a.vcxproj'])

    def test_unevaluated_reference_fails(self):
        self.project('a.vcxproj', '<ItemGroup><ProjectReference Include="$(External)/a.vcxproj"/></ItemGroup>')
        with self.assertRaisesRegex(ValueError, 'Non-literal'):
            inventory(self.root, ['a.vcxproj'])

    def test_external_reference_fails(self):
        self.project('a.vcxproj', '<ItemGroup><ProjectReference Include="../external.vcxproj"/></ItemGroup>')
        with self.assertRaises(ValueError):
            inventory(self.root, ['a.vcxproj'])

    def test_solution_dependencies(self):
        self.project('a.vcxproj')
        self.project('b.vcxproj')
        solution = self.root / 'test.sln'
        solution.write_text('''Project("{TYPE}") = "A", "a.vcxproj", "{A}"
 ProjectSection(ProjectDependencies) = postProject
  {B} = {B}
 EndProjectSection
EndProject
Project("{TYPE}") = "B", "b.vcxproj", "{B}"
EndProject
''')
        result = inventory(self.root, ['test.sln'])
        self.assertEqual(result['solution_dependencies'], [('a.vcxproj', 'b.vcxproj')])
        solution.write_text(solution.read_text().replace('{B} = {B}', '{UNKNOWN} = {UNKNOWN}'))
        with self.assertRaisesRegex(ValueError, 'Unknown solution dependency'):
            solution_projects(solution, self.root)


if __name__ == '__main__':
    unittest.main()
