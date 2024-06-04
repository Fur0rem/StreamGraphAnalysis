from distutils.core import setup, Extension

def main():
    setup(name="stream_graph_py",
          version="0.0.1",
          description="Python interface for the stream_graph_py C library function",
          author="<your name>",
          author_email="your_email@gmail.com",
          ext_modules=[Extension("stream_graph_py", ["stream_graph_module.c"])])

if __name__ == "__main__":
    main()
