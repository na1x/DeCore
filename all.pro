TEMPLATE = subdirs
SUBDIRS = decore \
          tests

CONFIG -= qt
CONFIG += ordered

tests.depends = decore
