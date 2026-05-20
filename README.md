## 소개
 
언리얼 엔진 5 환경에서 대표적인 그래프 탐색 알고리즘과 최소 신장 트리(MST) 알고리즘을 **실시간으로 시각화**하는 프로젝트입니다.
 
각 알고리즘의 탐색 과정, 방문 순서, 최종 경로를 뷰포트에 단계별로 확인할 수 있으며, 알고리즘 이론을 직관적으로 이해하는 데 초점을 맞추었습니다.

&nbsp;

---

&nbsp;

## ✨ 구현 예정 알고리즘
 
### 🗺️ 길찾기 (Pathfinding)

#### 탐색 (Graph Traversal) / 완전 탐색, 가중치 X
| 상태 | 알고리즘 | 특징 |
|:---:|---------|------|
| ✅ | **BFS** | 너비 우선 탐색 |
| ✅ | **DFS** | 깊이 우선 탐색 |

#### 최단 경로 (Shortest Path) / 가중치 기반, 최적해 보장
| 상태 | 알고리즘 | 특징 |
|:---:|---------|------|
| ⬜ | **Dijkstra** | 가중치 그래프에서의 최단 경로 |
| ⬜ | **Bellman–Ford** | 음수 가중치 지원 / 음수 사이클 감지 |

#### 휴리스틱 탐색 (Heuristic Search)  / 추정 비용 활용, 탐색 효율 우선
| 상태 | 알고리즘 | 특징 |
|:---:|---------|------|
| ✅ | **A\*** | 휴리스틱 기반 최적 경로 탐색 |
| ⬜ | **JPS** | 균일 격자 특화 A* 최적화 / 불필요한 노드 탐색 생략 |

&nbsp;

### 🌲 최소 신장 트리 (Minimum Spanning Tree)
| 상태 | 알고리즘 | 특징 |
|:---:|---------|------|
| ⬜ | **Kruskal** | 간선 정렬 기반 MST 구성 |
| ⬜ | **Prim** | 정점 기반 그리디 MST 구성 |

&nbsp;

---

&nbsp;
 
## 🛠️ 기술 스택
 
- **Engine** : Unreal Engine 5.7.4
- **Language** : C++ / Blueprint
- **Rendering** : Lumen / Nanite (UE5 기본 렌더 파이프라인)

&nbsp;

---

&nbsp;
 
## 🎬 시연 영상 / 스크린샷

> *(추후 GIF 또는 스크린샷 더 추가 예정)*
 <img width="2557" height="1388" alt="1" src="https://github.com/user-attachments/assets/bb6ec2e4-43a2-452e-a7a1-e2c56612ff14" />
 <img width="2555" height="1386" alt="1" src="https://github.com/user-attachments/assets/740efe1a-24ca-4983-a87c-2ba4b0e81823" />

 
---
