from setuptools import setup, find_packages


setup(
    name="xplaneconnect",
    version="1.0.0",
    python_requires=">=3",
    packages=find_packages("Python3/src"),
    package_dir={"": "Python3/src"},
    description="XPlaneConnect (XPC) facilitates communication to and from the XPCPlugin.",
    url="https://github.com/nasa/XPlaneConnect/",
)
