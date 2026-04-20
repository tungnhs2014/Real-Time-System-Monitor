# Start Here

This documentation set follows a design-led structure for a brownfield embedded product.

The repository already contains implementation, so the documentation intentionally separates:
- current implementation baseline
- target design direction
- chosen migration path

## Reading Paths

### 1. First-time reader
1. `docs/01-product/overview.md`
2. `docs/01-product/context-and-constraints.md`
3. `docs/01-product/requirements.md`
4. `docs/01-product/glossary.md`
5. `docs/02-solution/selected-solution.md`
6. `docs/02-solution/migration-strategy.md`

### 2. Architecture reviewer
1. `docs/02-solution/option-analysis.md`
2. `docs/02-solution/selected-solution.md`
3. `docs/02-solution/runtime-architecture-options.md`
4. `docs/02-solution/migration-strategy.md`
5. `docs/03-architecture/as-is-architecture.md`
6. `docs/03-architecture/target-architecture.md`
7. `docs/03-architecture/sad.md`
8. `docs/03-architecture/sdd.md`
9. `docs/03-architecture/interfaces-and-data-flow.md`
10. `docs/03-architecture/diagrams/`

### 3. BSP and Yocto reviewer
1. `docs/04-bsp-and-integration/hardware-overview.md`
2. `docs/04-bsp-and-integration/device-tree.md`
3. `docs/04-bsp-and-integration/ili9341-display-driver.md`
4. `docs/04-bsp-and-integration/xpt2046-touch-driver.md`
5. `docs/04-bsp-and-integration/yocto-layer.md`
6. `docs/04-bsp-and-integration/packaging-path.md`

### 4. Reviewer validating delivery readiness
1. `docs/05-verification/verification-strategy.md`
2. `docs/05-verification/requirement-traceability.md`
3. `docs/05-verification/test-cases.md`
4. `docs/05-verification/bring-up-checklist.md`
5. `docs/05-verification/known-limitations.md`

### 5. Build and handoff reader
1. `docs/06-operations/build-and-run.md`
2. `docs/06-operations/release-packaging.md`
3. `docs/06-operations/troubleshooting.md`

## Document Map
- `01-product/`: context, requirements, use cases
- includes glossary for brownfield terminology
- `02-solution/`: solution exploration and selected direction
  includes repository-level and runtime-level option analysis plus migration planning
- `03-architecture/`: as-is architecture, target architecture, design views, diagrams, ADR
- `04-bsp-and-integration/`: board support, device tree, drivers, Yocto, packaging path
- `05-verification/`: strategy, traceability, test cases, and bring-up guidance
- `06-operations/`: build, packaging, and troubleshooting guidance

## Repository Map
- `app/`: userspace Qt/QML application
- `yocto/meta-rpi-ili9341-bsp/`: active BSP and kernel-side integration
- `yocto/meta-system-monitor/`: product-side Yocto layer scaffold
- `private-docs/ai/`: internal AI workflow rules, not public design docs
