#!/usr/bin/env python3
# This file contains code generated with the assistance of Codex (OpenAI), an AI assistant.
# The generated code is provided as-is.

"""指定されたディレクトリ内の C++ ソースファイルを clang-format で整形します."""

import argparse
from pathlib import Path
import subprocess
import sys

SOURCE_SUFFIXES = {".cpp", ".hpp"}
TARGET_DIRECTORIES = ("vkwp", )


def parse_arguments() -> argparse.Namespace:
    """コマンドライン引数を解析します."""
    parser = argparse.ArgumentParser(
        description="対象ディレクトリ内の .hpp, .cpp ファイルを clang-format で整形します.",
    )
    parser.add_argument(
        "--dryrun",
        action="store_true",
        help="整形を実行せず, 処理対象のファイル一覧を表示します.",
    )
    return parser.parse_args()


def find_source_files(root_directory: Path) -> list[Path]:
    """対象ディレクトリ以下の C++ ソースファイルを取得します."""
    source_files: list[Path] = []
    for directory_name in TARGET_DIRECTORIES:
        target_directory = root_directory / directory_name
        source_files.extend(
            path
            for path in target_directory.rglob("*")
            if path.is_file() and path.suffix in SOURCE_SUFFIXES
        )

    return sorted(source_files)


def main() -> int:
    """対象ファイルの一覧表示, または clang-format による整形を実行します."""
    arguments = parse_arguments()
    root_directory = Path(__file__).resolve().parent
    source_files = find_source_files(root_directory)

    if not source_files:
        print("整形対象の .hpp, .cpp ファイルはありません.")
        return 0

    if arguments.dryrun:
        for source_file in source_files:
            print(source_file.relative_to(root_directory))
        print(f"処理対象は {len(source_files)} 個のファイルです.")
        return 0

    failed_files: list[Path] = []
    for source_file in source_files:
        print(f"Formatting: {source_file.relative_to(root_directory)}")
        result = subprocess.run(
            ["clang-format", "-i", str(source_file)],
            check=False,
        )
        if result.returncode != 0:
            failed_files.append(source_file)

    if failed_files:
        print(
            f"{len(failed_files)} 個のファイルの整形に失敗しました.",
            file=sys.stderr,
        )
        return 1

    print(f"{len(source_files)} 個のファイルを整形しました.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
