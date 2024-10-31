from setuptools import setup, find_packages


setup(
    name="hello",
    version="0.1",
    packages=find_packages(),
    entry_points={
        "console_scripts": [
            "hello=hello.main:hello_world",
        ],
    },
)
