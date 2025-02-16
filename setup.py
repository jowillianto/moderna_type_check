from setuptools import setup, find_namespace_packages
from setuptools.dist import Distribution

class BinaryDistribution(Distribution):
    def is_pure(self):
        return False
    def has_ext_modules(self) -> bool:
        return True

setup(
    name="moderna-type-check",
    version="1.0.0",
    description="A Custom Type Checker for pipeline like application",
    url="https://github.com/jowillianto/moderna_type_check",
    author="Jonathan Willianto",
    author_email="jo.will@calici.co",
    license="MIT",
    packages=[
        f"moderna.{pkg_name}" for pkg_name in find_namespace_packages("moderna")
    ],
    install_requires = [
      "typing_extensions>=4.7.1"  
    ],
    include_package_data=True,
    distclass = BinaryDistribution
)
