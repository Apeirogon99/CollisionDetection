# CollisionDetection

CollisionDetection은 Array, KD-Tree, QuadTree에 대한 일부 충돌 검사에 대한 결과입니다.
테스트 환경 자체가 넓게 분산된 예제이기 때문에 상황에 따라 다를 수 있습니다.

주의 : QuadTree는 이득우 강사님의 QuadTree를 C++로 변환한 것입니다.

# 📋 목차
- [문제 상황](#문제-상황)
- [사용법](#사용법)
- [결과 예시](#결과)
- [결론](#결론)

# 문제 상황

근접 공격, 원거리 공격, 스킬 등 모든 공격을 논타겟으로 제작하고자 하였습니다. </br>

모든 액터들을 가시거리로 처리하고 있었으며 기반이 다져진 상태였습니다. </br>
고민 하던 중 특정 지점 근처의 객체를 빠르게 찾거나 일정 범위 내 객체를 검색 속도에 이점이 있었습니다. </br>
이는 지금까지 만들어둔 논 타겟 스킬, 공격등 매우 어울린다고 생각하였습니다. </br>

이러한 이유를 근거로 KD-Tree를 사용해 만들었습니다. </br>
하지만 정말 맞는 선택인가에 대한 의문이 있었고 테스트 환경을 만들어서 비교하였습니다. </br>

# 사용법
클라이언트를 SFML 3.0을 사용하였기에 ThirdParty 폴더를 만들어서 lib, include를 넣어야 합니다. </br>

1번 = 객체가 1000씩 늘어남 </br>
2번 = 객체가 1000씩 줄어듬 </br>
3번 = 디버그 시각화 (KD-Tree, Quad-Tree) </br>

왼쪽 마우스 클릭 = 공격 활성화 </br>
Q = 공격의 크기를 10 증가 </br>
W = 공격의 크기를 10 감소 </br>

# 결과
### 테스트 환경
<img width="609" height="608" alt="image" src="https://github.com/user-attachments/assets/f57e1e56-e3d3-4f8b-ac03-6743e98e834b" />

### 빌드 결과
<img width="700" height="429" alt="image" src="https://github.com/user-attachments/assets/bd500398-9914-466b-8119-e9818897fd24" />

### 검색 결과
<img width="702" height="427" alt="image" src="https://github.com/user-attachments/assets/fa8401ec-d43f-47d5-be46-e3223c90cef5" />

# 결론
- 동적 객체의 재배치 비용
  - 노드를 삽입, 삭제, 이동하면서 규모에 따라 재배치 시간이 O(n log n) 복잡도로 비용이 커져 부담
  - 동적인 움직임을 계속 업데이트 해야한다면 KD-Tree보다는 Quad-Tree, Array가 좋은 선택
- 데이터 규모
  - 많은 수의 충돌 검사를 처리해야한다면 KD-Tree, Quad-Tree가 공간 분할 구조이기에 유리함
  - 적은 수의 경우 충돌 검사를 처리시 Array가 빌드 시간이 없고 캐시 효율성으로 인해 더 빠를 수 있음


