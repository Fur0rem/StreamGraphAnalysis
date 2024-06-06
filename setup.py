from setuptools import Extension, setup

setup(
    ext_modules=[
        Extension(
            name="stream_graph_py",  # as it would be imported
                               # may include packages/namespaces separated by `.`

            sources=["src/stream_graph_py_module.c"], # all sources are compiled into a single binary file
        ),
    ]
)