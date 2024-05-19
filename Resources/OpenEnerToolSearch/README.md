Openenertoolsearch is a tool to download git repos for energy management and search them with recoll.

Adapt project_list.csv as you wish, e.g. use https://github.com/hblasum/energy-landscape

Questions: holger.blasum AT gmail DOT com

$ mkdir checkout
$ sudo apt-get install recoll python3-waitress python3-recoll aspell libapache2-mod-wsgi-py3
$ git clone https://framagit.org/medoc92/recollwebui
$ recollwebui/webui-standalone.py -c /home/openenertoolsearch/openenertoolsearch/.recoll

Experimental installation at: http://openenertoolsearch.blasum.net/recoll/

Notes:

WSGI may need removal of mod_python
dpkg --purge libapache2-mod-python
 

