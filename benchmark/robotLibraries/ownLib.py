import os
import math
#	  Errors which can be used:
#	  raise AssertionError('No scalarfield in input-file')
#	  raise RuntimeError('Different meshsize, no mapping possible')
#	  raise ValueError('Different meshsize, no mapping possible')	



def run_command(command):
	#ATTENTION: every command will be started in a new shell -> FIX: combine more commands with ";"!
	os.system(command)
	print 'Running the command: ' + command
	
def run_script(script):
	os.system('sh ' + script)
	print 'Running the script: ' + script
	
def publish_file(filePath):
	"""Publish the file to log.html"""
	print '*HTML* <a href="' + filePath + '"> ' + filePath + ' </a>'
