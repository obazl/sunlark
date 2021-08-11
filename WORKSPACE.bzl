load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")  # buildifier: disable=load
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "maybe")  # buildifier: disable=load

all_content = """filegroup(name = "all", srcs = glob(["**"]), visibility = ["//visibility:public"])"""

def cc_fetch_repos():

    maybe(
        git_repository,
        name = "sealark",
        remote = "https://github.com/obazl/sealark",
        # commit = "b1c40e1de81913a3c40e5948f78719c28152486d",
        # shallow_since = "1605101351 -0800"
        branch = "dev"
    )

    maybe(
        git_repository,
        name = "rules_cc",
        remote = "https://github.com/bazelbuild/rules_cc",
        commit = "b1c40e1de81913a3c40e5948f78719c28152486d",
        shallow_since = "1605101351 -0800"
        # branch = "master"
    )

    maybe(
        http_archive,
        name = "rules_foreign_cc",
        sha256 = "e14a159c452a68a97a7c59fa458033cc91edb8224516295b047a95555140af5f",
        strip_prefix = "rules_foreign_cc-0.4.0",
        url = "https://github.com/bazelbuild/rules_foreign_cc/archive/0.4.0.tar.gz",
    )

    # http://www.throwtheswitch.org/unity
    maybe(
        http_archive,
        name = "unity",
        urls = [
            "https://github.com/ThrowTheSwitch/Unity/archive/refs/tags/v2.5.2.zip",
        ],
        strip_prefix = "v2.5.2",
        build_file_content = all_content,
        workspace_file_content = "workspace( name = \"unity\" )"
    )

    ## for s7
    maybe(
        http_archive,
        name = "libffi",
        url = "https://github.com/libffi/libffi/archive/refs/tags/v3.4.2.zip",
        strip_prefix = "libffi-3.4.2",
        build_file_content = all_content,
        # sha256 = "72fba7922703ddfa7a028d513ac15a85c8d54c8d67f55fa5a4802885dc652056",
        # build_file = "@//bzl/external:libffi.BUILD",
        ## the zip version requires use of autogen
        #url = "https://github.com/libffi/libffi/archive/v3.3.zip",
        # type = "zip",
        # sha256 = "60b64c656520f986ec7bd2a6dc61e800848c97872f8f5132c5f753d9c205c358",
)

