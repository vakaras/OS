from setuptools import setup, find_packages

setup(name='$(name)',
      version='0.1',
      author='$(author)',
      author_email='$(author)@localhost',
      packages=find_packages(),
      install_requires=[
          'distribute',
      ],
      url='',
      license='GPL',
      description='$(name).',
      long_description=open('README.rst').read(),
      )
