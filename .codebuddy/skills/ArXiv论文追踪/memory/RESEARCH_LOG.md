# ArXiv Research Log

---

### [2026-05-28] Physics Is All You Need? A Case Study in Physicist-Supervised AI Development of Scientific Software
- **Authors**: Nhat-Minh Nguyen
- **Link**: https://arxiv.org/abs/2605.30353v1
- **PDF**: https://arxiv.org/pdf/2605.30353v1
- **Summary**: 物理学家监督 AI Coding Agent（Claude Code）开发科学软件的定量案例研究。Agent 能自主解决 10/15 个监督事件，但存在"治标不治本"问题——花费 33/57 个 session 调整系数却未重新评估错误架构。结论：监督设计比模型能力更关键。与研究方向（AI Agent 代码理解局限）高度相关。

---

### [2026-05-28] RADAR: Risk-Aware Automated Code Review at Meta
- **Authors**: Adams et al. (Meta)
- **Link**: https://arxiv.org/abs/2605.30208v1
- **PDF**: https://arxiv.org/pdf/2605.30208v1
- **Summary**: Meta 部署的多阶段风险分层自动代码审查管道，已审查 53.5 万+ diff，回滚率为非 RADAR 的 1/3，生产事故率 1/50，中位关闭时间减少 330%。对 AI Agent + 代码审查方向有重要参考价值。

---

### [2026-05-28] EvoRepair: Self-Evolving Vulnerability Repair through Experience
- **Authors**: Hu et al.
- **Link**: https://arxiv.org/abs/2605.30105v1
- **PDF**: https://arxiv.org/pdf/2605.30105v1
- **Summary**: 首个经验自演进漏洞修复 Agent 框架，通过质量感知评分循环积累领域知识。PATCHEVAL 达 93.47%，SEC-bench 达 87.00%。对 AI Agent + 代码修复研究有直接参考价值。

---

### [2026-05-28] Projectional Decoding: Towards Semantics-Aware Code Generation
- **Authors**: Chen et al.
- **Link**: https://arxiv.org/abs/2605.30054v1
- **PDF**: https://arxiv.org/pdf/2605.30054v1
- **Summary**: 在 LLM 生成过程中同时维护图模型表示，实现增量语义验证和错误检测，提供可验证的语义正确性保证。与程序生成/LLM 代码生成研究直接相关。

---

### [2026-05-28] REPOT: Recoverable Programs of Thoughts
- **Authors**: Parsa Mazaheri
- **Link**: https://arxiv.org/abs/2605.30052v1
- **PDF**: https://arxiv.org/pdf/2605.30052v1
- **Summary**: 通过确定性验证重放，从已验证前缀恢复失败的 PoT 推理链，仅需额外一次 LLM 调用。PuzzleZoo-775 达 96.9%（PoT 基线仅 86.3%）。

---

### [2026-05-28] The Rise of Software-Defined Vehicles
- **Authors**: Liotou et al.
- **Link**: https://arxiv.org/abs/2605.30001v1
- **PDF**: https://arxiv.org/pdf/2605.30001v1
- **Summary**: 软件定义车辆（SDV）综述，涵盖从分布式 ECU 到集中式计算平台的架构演进、面向服务架构、中间件、AI 机制和云基础设施。

---

### [2026-05-28] Agora: Autonomous Bug Detection in Consensus Protocols
- **Authors**: Liu et al.
- **Link**: https://arxiv.org/abs/2605.29910v1
- **PDF**: https://arxiv.org/pdf/2605.29910v1
- **Summary**: 领域感知多智能体框架，在 Raft、EPaxos、HotStuff、BullShark 中发现 15 个未知协议级逻辑 bug，现有 LLM Agent 无法检出。对多智能体协作 + 程序验证研究有参考价值。

---

### [2026-05-28] Statistical Artifacts in Quantum Error Mitigation Benchmarks
- **Authors**: Köster, Mauerer
- **Link**: https://arxiv.org/abs/2605.29872v1
- **PDF**: https://arxiv.org/pdf/2605.29872v1
- **Summary**: 评审 81 篇量子错误缓解（QEM）论文，仅 25% 使用推断统计，指出参数敏感性和时序漂移导致的统计伪影。

---

### [2026-05-28] TagDebt: A Bot for Technical Debt Management
- **Authors**: Biazotto et al.
- **Link**: https://arxiv.org/abs/2605.29869v1
- **PDF**: https://arxiv.org/pdf/2605.29869v1
- **Summary**: GitHub 集成机器人，自动为 issue 打 SATD（自承认技术债务）标签，16 名实践者评价有用且易用。

---

### [2026-05-28] TRAILS: Testing Requirements and AI for Inferring code correctness from LLM-generated Specifications
- **Authors**: Tambon Florian, Papadakis Mike
- **Link**: https://arxiv.org/abs/2605.29822v1
- **PDF**: https://arxiv.org/pdf/2605.29822v1
- **Summary**: 通过规范生成测试、LLM 评估输入输出对是否符合规范，推断代码正确性，MCC 最高提升 39%。不直接推理代码本身，而是借助规范桥接。与程序理解和代码生成验证研究直接相关。
