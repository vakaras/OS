from setuptools import setup, find_packages

setup(name='pyemu',
      version='0.1',
      author='vakaras',
      author_email='vakaras@localhost',
      packages=find_packages(),
      install_requires=[
          'distribute',
      ],
      url='',
      license='GPL',
      description='pyemu.',
      long_description=open('README.rst').read(),
      )
