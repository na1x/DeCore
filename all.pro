TEMPLATE = subdirs
SUBDIRS = decore \
          tests

CONFIG += ordered
app.depends = tests decore
