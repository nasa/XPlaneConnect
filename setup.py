from setuptools import setup, find_packages


setup(
    name="xplaneconnect",
    version="1.3rc6",
    python_requires=">=3",
    packages=find_packages("Python3/src"),
    package_dir={"": "Python3/src"},
    description="XPlaneConnect (XPC) facilitates communication to and from the XPCPlugin. Note: To use the XPC package, you must first install the XPCPlugin. See https://github.com/nasa/XPlaneConnect/wiki/Getting-Started for instructions on installing the XPCPlugin",
    url="https://github.com/nasa/XPlaneConnect/",
    project_urls={
        "Bug Reports": "https://github.com/nasa/xplaneconnect/issues",
        "Organization": "https://www.nasa.gov/content/diagnostics-prognostics",
        "Source": "https://github.com/nasa/xplaneconnect",
        "Documentation": "https://github.com/nasa/XPlaneConnect/wiki"
    }
)
