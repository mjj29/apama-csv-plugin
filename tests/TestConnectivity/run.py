from pysys.constants import *
from apama.basetest import ApamaBaseTest
from apama.correlator import CorrelatorHelper
import re

class PySysTest(ApamaBaseTest):
	def execute(self):
		correlator = CorrelatorHelper(self, name='testcorrelator')
		correlator.start(logfile='testcorrelator.log', config=self.input+'/config.yaml')
		correlator.injectEPL(filenames=['ConnectivityPluginsControl.mon'], filedir=PROJECT.APAMA_HOME+'/monitors')
		correlator.injectEPL(filenames=['ConnectivityPlugins.mon'], filedir=PROJECT.APAMA_HOME+'/monitors')
		correlator.injectEPL(filenames=['test.mon'])
		correlator.flush() 
		self.waitForSignal(file='testcorrelator.log', expr='Round-trip OK', condition='>=4')

	def validate(self):
		self.assertGrep('testcorrelator.log', expr='ERROR', contains=False)
		self.assertLineCount('testcorrelator.log', expr='Round-trip OK', condition='==4')
		self.assertDiff('tsv-before.txt', 'tsv-before.txt')
		self.assertDiff('tsv-after.txt', 'tsv-after.txt')
		self.assertDiff('csv-before.txt', 'csv-before.txt')
		self.assertDiff('csv-after.txt', 'csv-after.txt')
