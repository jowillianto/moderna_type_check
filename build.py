#!python3
import subprocess
from concurrent.futures import ThreadPoolExecutor
from typing_extensions import List, Literal
import argparse


def get_args(
    platform: Literal["linux/amd64", "linux/arm64"], version: str
) -> List[str]:
    return [
        "docker",
        "build",
        f"--platform={platform}",
        "-o",
        "dist",
        "-f",
        "dockerfile.export",
        "--build-arg",
        f"PYTHON_VERSION=3.{version}",
        ".",
    ]


subprocess_args = [
    get_args(platform, str(version))  # type: ignore
    for version in range(7, 13)
    for platform in ["linux/amd64", "linux/arm64"]
]


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--parallel",
        type=bool,
        default=False,
        help="Use False to see more clearly",
        required=False,
    )
    parser.add_argument(
        "--version", type=int, help="Python version to build", required=False
    )
    parallel = parser.parse_args().parallel
    version = parser.parse_args().version
    if version is not None:
        subprocess.run(get_args("linux/amd64", str(version)))
        subprocess.run(get_args("linux/arm64", str(version)))
    elif parallel:
        with ThreadPoolExecutor() as t:
            t.map(subprocess.run, subprocess_args)
    else:
        for args in subprocess_args:
            subprocess.run(args)
