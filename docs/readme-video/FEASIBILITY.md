# README에서 YouTube 영상 재생 — 실현 가능성 조사

조사일: 2026-08-27 · 대상: github.com에서 렌더링되는 저장소 마크다운(README 포함)

## 결론 한 줄

**README 안에서 YouTube를 그대로 재생시키는 것은 불가능하다.** 다만 "README를 보는
화면에서 영상이 재생된다"는 목표 자체는, GitHub에 직접 업로드한 영상으로는 **가능하다**.

## 1. 실측 결과

`agnesAqr/wildbound` 브랜치에 프로브 마크다운을 올려 GitHub 렌더링 결과를 직접 측정했다.

| 입력 | 렌더링 결과 |
|---|---|
| `<iframe src="youtube.com/embed/...">` | 태그·내부 텍스트까지 **전부 삭제** |
| `<video src="https://example.com/x.mp4">` (외부 호스팅) | 태그·내부 텍스트까지 **전부 삭제** |
| `<embed>` | **전부 삭제** |
| `<script>` / `<style>` | **전부 삭제** |
| `<object>` / `<audio>` | 태그 삭제, 내부 텍스트만 남음 |
| 일반 마크다운 링크 | 정상 |
| `<video src="https://github.com/user-attachments/assets/...">` | 화면에 아무것도 안 나옴 (아래 3절 주의) |
| `https://github.com/user-attachments/assets/<uuid>` (단독 줄) | **HTML5 플레이어로 치환됨** |
| 외부 이미지/애니메이션 SVG | `camo.githubusercontent.com` 프록시 URL로 치환되어 정상 표시 |

교차 검증: `Skyvern-AI/skyvern`의 README `# Demo` 섹션은 원본 마크다운에 URL 한 줄만
있는데, 렌더링된 페이지에서는 그 자리에 `skyvern_demo_video_v2.1.mp4` 캡션이 달린
플레이어가 나온다. 같은 페이지의 다른 일반 링크들은 링크 그대로 남는다.

## 2. 왜 iframe이 막히는가

GitHub은 마크다운 렌더링 시 허용 목록(allowlist) 기반으로 HTML을 sanitize한다.
`<iframe>`, `<script>`, `<style>`, `<embed>`는 목록에 없어 제거된다. `<style>`과
class/id가 제거되므로 CSS 트릭도, 클릭 상호작용도 만들 수 없다. 우회 경로는 없다.

## 3. 공식 문서가 못박은 부분

GitHub Docs, "About anonymized URLs":

> Videos also get anonymized URLs with the same format as image URLs, but are not
> processed through Camo. This is because **GitHub does not support externally hosted
> videos**, so the anonymized URL is a link to the uploaded video hosted by GitHub.

즉 외부 호스팅 영상(YouTube 포함)은 지원 대상이 아니라고 명시돼 있다. 재생되는 영상은
**GitHub에 업로드된 자산뿐**이다.

GitHub Docs, "Attaching files":
- 지원 영상 포맷: `.mp4`, `.mov`, `.webm` (호환성 위해 H.264 권장)
- 용량: 이미지/GIF 10MB / 영상 무료 플랜 10MB, 유료 플랜 100MB / 기타 파일 25MB
- 이미지·영상 파일 타입은 "supported in all contexts"

주의할 함정: **저장소에 커밋한 영상 파일은 렌더링되지 않는다.** 상대경로도,
`raw.githubusercontent.com` 절대경로도 링크로만 표시된다. 반드시 업로드 자산 URL이어야 한다.

## 4. 정책 리스크

- **GitHub AUP §9 (Excessive Bandwidth Use)**: 대역폭 사용이 과도하다고 판단되면 계정
  정지, 파일 호스팅 스로틀, 저장소 삭제까지 가능. GitHub을 영상 CDN처럼 쓰는 설계는 위험.
- **GitHub AUP §10 (Advertising)**: README에 넣는 이미지·링크는 "호스팅 중인 프로젝트와
  관련" 있어야 한다.
- **YouTube 약관**: 권리를 가진 콘텐츠만 다운로드·재사용·재배포할 수 있고, 임베드는 공식
  iframe 플레이어를 쓰도록 요구한다. 따라서 **남의 유튜브 영상을 받아서 GIF/mp4로 변환해
  GitHub에 올리는 방식은 약관 위반**이다. 본인 소유 영상이면 문제없다.

## 5. 현실적인 선택지

| 방식 | README 내 재생 | 소리 | 컨트롤 | 비고 |
|---|---|---|---|---|
| A. 썸네일 이미지 + 유튜브 링크 | ✗ (클릭 시 이동) | – | – | 사실상 업계 표준, 비용 0 |
| B. GitHub에 영상 직접 업로드 | **✓** | ✓ | ✓ | 본인 영상만. 10/100MB 제한 |
| C. GIF·애니메이션 SVG 프리뷰 | 자동재생 | ✗ | ✗ | camo 캐시로 프레임 단위 제어 불가 |
| D. GitHub Pages에 iframe 페이지 | ✗ (외부 이동) | ✓ | ✓ | 정석적인 우회 |

## 6. 만든다면 무엇을 만들 수 있나

- **썸네일 카드 생성기 (A 자동화)**: 유튜브 URL을 넣으면 재생버튼이 합성된 썸네일과
  마크다운 스니펫을 뽑아준다. 약관·정책 리스크 없음. 가장 안전하고 즉시 유용.
- **영상 최적화 도우미 (B 보조)**: 데모 영상을 H.264로 인코딩해 플랜별 용량 한도 아래로
  맞춰주고, 업로드 후 넣을 마크다운을 안내한다.
- **하이라이트 GIF/SVG 생성기 (C)**: 본인 소유 영상에서 짧은 루프를 뽑아 README 상단 배치.

## 7. 아직 확인 못한 것

- **Release 자산 URL(`/releases/download/...`)이 플레이어로 치환되는지**는 미검증.
  치환된다면 공식 REST API로 업로드를 자동화할 수 있어 B안 자동화의 핵심이 된다.
  검증법: 테스트 저장소에 mp4를 릴리스 자산으로 올리고 URL을 단독 줄로 렌더링해 본다.
- user-attachments 업로드용 **공식 REST API는 확인되지 않았다.** 문서상 경로는 웹
  에디터 드래그앤드롭뿐이다.
- YouTube 약관 원문은 이 환경의 네트워크 정책으로 직접 열지 못했다. 위 4절의 YouTube
  관련 내용은 2차 출처 기반이므로, 실제 진행 전 원문 확인이 필요하다.
