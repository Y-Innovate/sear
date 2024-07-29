# -*- coding: utf-8 -*-
from setuptools import setup

packages = \
['pyracfu']

package_data = \
{'': ['*']}

setup_kwargs = {
    'name': 'pyRACFu',
    'version': '1.0a1',
    'description': 'Python interface to RACF using RACF Callable Services.',
    'long_description': '# openSAF\nSource code for "openSAF" or whatever we end up calling it.\n',
    'author': 'Joe Bostian',
    'author_email': 'jbostian@ibm.com',
    'maintainer': 'Leonard J. Carcaramo Jr',
    'maintainer_email': 'lcarcaramo@ibm.com',
    'url': 'https://github.com/ambitus/pyracf',
    'packages': packages,
    'package_data': package_data,
    'python_requires': '>=3.10',
}
from build_extension import *
build(setup_kwargs)

setup(**setup_kwargs)
