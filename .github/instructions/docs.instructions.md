---
description: "Use when editing FrontierX Markdown documentation, project status, plans, handoffs, compatibility notes, or modification notices."
applyTo: "**/*.md"
---

# FrontierX Documentation Instructions

- Write repository documentation in English.
- Separate current, verified behavior from plans and hypotheses. Include a date for status snapshots.
- Identify evidence as source review, compile result, automated execution, synthetic runtime,
  Windows runtime or retail runtime.
- Keep active port architecture in `docs/PROJECT.md`; keep detailed Windows restoration state in
  `HANDOFF.md` and acceptance evidence in `RETAIL_TEST_MATRIX.md`.
- Keep `AGENTS.md` limited to durable rules, a concise current-state summary, the next accepted
  milestone and document entry points. Put component histories, commands, measured outputs and
  unresolved checks in focused documents under `docs/`.
- Add focused documents under `docs/` rather than accumulating unrelated notes at the repository
  root. Preserve upstream documents and links needed for provenance.
- Update commands in `README.md`, `docs/PROJECT.md`, and `AGENTS.md` together when a supported build
  or test workflow changes.
- Keep `MODIFICATIONS.md` dated and attributed for material changes before distribution.
- For each material change, record the exact target/configuration and host/compiler built, what
  actually executed, the input provenance, an observable result and every relevant Windows/retail/
  ABI/hardware check still open. Do not use a repeated global CTest total as component evidence.
- Never alter, paraphrase as controlling text, or replace `Conquest Source License.txt`.
- Do not paste raw decompiler output, disassembly dumps, retail data, personal paths, secrets, or
  copyrighted manuals into documentation.
- Use relative links for repository files and verify that each new link resolves.

Do not copy GeneralsX names, paths, commands, completed features, or project-specific policies into
FrontierX documentation. It is a design reference, not project evidence.
