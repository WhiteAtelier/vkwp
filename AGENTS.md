# Agents Instructions

## 総合

- ソースコードのコメントや、その他ドキュメントは、基本的に日本語を使うものとします。
- ただし、クラス名や関数名などに対するコメントなどでは、クラス名や関数名に使われている英語は、そのまま英語として使います。
- 句点は "."、読点は "," とします。また句読点の後ろに文が続く場合は、その後ろには必ずスペースを空けてください。
    - 例: `ソースコードのコメントや, その他のドキュメントは, 基本的に日本語を使うものとします. しかし, 例外はあります.`
- コメント文や markdown など、改行について挙動上の意味を持たない場合は、基本的に1行に1文とします。
- 英単語と日本語の間にはスペースを置いて空間をあけてください。
    - 例: `この関数は args を指定することで, 戻り値が void になるオーバーロードがあります. `

## C++

- オブジェクト変数を引数付きで初期化する場合は, 可能な限り `{}` を使ってください.
    - 例: `Hoge hoge{ "aaa" };`
- デフォルトコンストラクタを呼び出すだけの場合は, 省略可能な `{}` を記述しないでください.
    - ダメな例: `std::vector<RawTempoChange> raw_tempo_changes{};`
    - 良い例: `std::vector<RawTempoChange> raw_tempo_changes;`
    - ダメな例: `smf::MidiFile midi_file{};`
    - 良い例: `smf::MidiFile midi_file;`
- 整数型, 浮動小数点型, 文字型, 真偽値型などのプリミティブ型は, 代入初期化を使用してください.
    - 例: `std::int32_t tick = 0;`
    - 例: `double seconds = 0.0;`
    - 例: `std::uint8_t velocity = 0;`
- 通常の整数値には `int` などの組み込み整数型をできるだけ使用せず, `<cstdint>` の固定幅整数型を使用してください.
    - 値域と符号に応じて `std::int8_t`, `std::uint8_t`, `std::int16_t`, `std::uint16_t`, `std::int32_t`, `std::uint32_t`, `std::int64_t`, `std::uint64_t` を選択してください.
    - 固定幅が不要な場合は, `std::int_fast32_t`, `std::uint_fast32_t`, `std::int_least32_t`, `std::uint_least32_t` などを使用しても構いません.
    - 外部 API の型, 戻り値の型, 規格で型が決まっている値との互換性が必要な場合は, その型に合わせて `int` などを使用しても構いません.
- 整数型や浮動小数点型などの数値変数では, `auto` による型推論を避け, 型を明示してください.
    - ダメな例: `auto time_signature_numerator = 4;`
    - 良い例: `std::int32_t time_signature_numerator = 4;`
    - ダメな例: `const auto default_bpm = 120.0;`
    - 良い例: `constexpr double default_bpm = 120.0;`
- 数値以外で型推論が明確な代入の場合は, `auto` を使うようにしてください.
    - ダメな例: `std::string str2 = str1;`
    - 良い例: `auto str2 = str1;`
- `move` が可能な場合はできる限り move してください。
- インクルードガードには `#ifdef ... #define ... #endif` のタイプを使用します。
    - インクルードガード名は、そのヘッダファイルのディレクトリ構造を参考にし、include ディレクトリ以下の名前とヘッダファイル名を大文字スネークで連結し、 `#define VKWP_HEADER_NAME_HPP` のようにします。
    - ディレクトリ名はスネークケースが基本で、アンダースコアはそのままインクルードガードの define にも反映します。
        - 例: vkwp/foo_bar.hpp -> `VKWP_FOO_BAR_HPP`
- クラス定義について、クラス名は基本的にファイル名と一致させ、しかし PascalCase とします。
    - 例: foo_bar.hpp -> `class FooBar {...};`
- クラス定義について、基本的にすべてのクラスに、デフォルトコンストラクタ、コピーコンストラクタ、ムーブコンストラクタ、コピー代入、ムーブ代入、デストラクタを用意して、 delete の場合でも明示的に delete します。
    - 5-rules を明記します
- クラス名は PascalCase を使います。
- 関数名は動詞で始める形として、 `lower Pascal Case` とします。
- 変数名は snake_case を使います。
- public ではないメソッドの場合は `lower Pascal Case` とし、 prefix に `_` を使います。
    - 例: `void _createInstance()`
- public ではないメンバ変数の場合は `snake_case` とし、 suffix に `_` を使います。
    - 例: `int hoge_huga_;`
- 関数定義の引数は、関数内で move することがない限り、リテラルであっても const を適用します。
    - 例: `void foobar(const int argc) { ... }`
- そしてその関数宣言についても、仕様的には不要ですが、同じく const を適用します。
    - 例: `void foobar(const int argc);`
- クラスメソッドの定義において、クラスメンバにアクセスする際は、必ず `this->` を明記します。
- 関数定義における名前空間は、ひとつひとつについて完全名で記してください。
    - ダメな例: `namespace roah::hoge { void foobar() { ... /* 定義 */ ... } }`
    - 良い例: `void roah::hoge::foobar() { ... /* 定義 */ ... }`
- 全体のフォーマットは clang-format で実行されるため、一括でかけるので AI は気にしないでいいです。
- イテレータについて、変数名は基本的に `iter` を使ってください。
- 文末は CRLF でそろえてください。
- 必ず、 if や for, while などのスコープを持つものは、その中身が１行だったとしても、必ず `{}` によるスコープ宣言をしてください。
    - ダメな例: `if (hoge) foobar = 1;`
    - 良い例: `if (hoge) { foobar = 1; }`

## GIT

- コミットは指示があった場合にのみ実行してください
    - コミットメッセージは日本語で、1行目簡潔に、2行目開けて、3行目以降詳しく書く

## Documentation

- このプロジェクトはドキュメントコメントとして doxygen に対応します.
- 詳しくは doxygen.md を参照してください.
