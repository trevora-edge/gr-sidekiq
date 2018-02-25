#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Sun Feb 25 17:55:40 2018
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import digital
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.qtgui import Range, RangeWidget
from optparse import OptionParser
import numpy
import sidekiq
import sys
import threading
import time
from gnuradio import qtgui


class top_block(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Top Block")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Top Block")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Variables
        ##################################################
        self.variable_function_probe_0 = variable_function_probe_0 = 0
        self.sample_rate = sample_rate = 1e6
        self.alpha_range = alpha_range = .1
        self.vector_length = vector_length = 1
        self.variable_qtgui_label_0 = variable_qtgui_label_0 = variable_function_probe_0
        self.source_min_output_buffer = source_min_output_buffer = 2**14*2
        self.rrc_filter_taps = rrc_filter_taps = firdes.root_raised_cosine(1, sample_rate, sample_rate/2.0, alpha_range, 128)
        self.max_sample_rate = max_sample_rate = 56e6
        self.gaussian_filter_taps = gaussian_filter_taps = firdes.gaussian(1.0, 2.0, alpha_range, 128)
        self.gain = gain = 55
        self.center_freq = center_freq = 915e6
        self.burst_length_range = burst_length_range = 20
        self.burst_interval_range = burst_interval_range = 100
        self.bandwidth = bandwidth = sample_rate

        ##################################################
        # Blocks
        ##################################################
        self._sample_rate_range = Range(0.1e6, max_sample_rate, 0.1e6, 1e6, 200)
        self._sample_rate_win = RangeWidget(self._sample_rate_range, self.set_sample_rate, 'Sample Rate', "counter_slider", float)
        self.top_grid_layout.addWidget(self._sample_rate_win, 1,0,1,1)
        self._gain_range = Range(1, 76, 1, 55, 200)
        self._gain_win = RangeWidget(self._gain_range, self.set_gain, 'Gain', "counter_slider", float)
        self.top_grid_layout.addWidget(self._gain_win, 0,1,1,1)
        self._center_freq_range = Range(100e6, 6000e6, 1e6, 915e6, 200)
        self._center_freq_win = RangeWidget(self._center_freq_range, self.set_center_freq, 'Frequency', "counter_slider", float)
        self.top_grid_layout.addWidget(self._center_freq_win, 0,0,1,1)
        self.blocks_probe_rate_0 = blocks.probe_rate(gr.sizeof_gr_complex*1, 500.0, 0.15)
        self._bandwidth_range = Range(0.1e6, max_sample_rate, 0.1e6, sample_rate, 200)
        self._bandwidth_win = RangeWidget(self._bandwidth_range, self.set_bandwidth, 'Bandwidth', "counter_slider", float)
        self.top_grid_layout.addWidget(self._bandwidth_win, 1,1,1,1)
        self._variable_qtgui_label_0_tool_bar = Qt.QToolBar(self)

        if None:
          self._variable_qtgui_label_0_formatter = None
        else:
          self._variable_qtgui_label_0_formatter = lambda x: x

        self._variable_qtgui_label_0_tool_bar.addWidget(Qt.QLabel('Rx Rate'+": "))
        self._variable_qtgui_label_0_label = Qt.QLabel(str(self._variable_qtgui_label_0_formatter(self.variable_qtgui_label_0)))
        self._variable_qtgui_label_0_tool_bar.addWidget(self._variable_qtgui_label_0_label)
        self.top_grid_layout.addWidget(self._variable_qtgui_label_0_tool_bar, 5,0,1,1)


        def _variable_function_probe_0_probe():
            while True:
                val = self.blocks_probe_rate_0.rate()
                try:
                    self.set_variable_function_probe_0(val)
                except AttributeError:
                    pass
                time.sleep(1.0 / (10))
        _variable_function_probe_0_thread = threading.Thread(target=_variable_function_probe_0_probe)
        _variable_function_probe_0_thread.daemon = True
        _variable_function_probe_0_thread.start()

        self.sidekiq_sidekiq_tx_0 = sidekiq.sidekiq_tx(sample_rate, gain, center_freq, bandwidth, 1, bool(True),
                0, 1020, ())

        self.digital_psk_mod_0 = digital.psk.psk_mod(
          constellation_points=4,
          mod_code="gray",
          differential=True,
          samples_per_symbol=2,
          excess_bw=1,
          verbose=False,
          log=False,
          )
        (self.digital_psk_mod_0).set_min_output_buffer(65536)
        self._burst_length_range_range = Range(1, 1000, .1, 20, 200)
        self._burst_length_range_win = RangeWidget(self._burst_length_range_range, self.set_burst_length_range, 'Burst Length', "counter_slider", float)
        self.top_grid_layout.addWidget(self._burst_length_range_win, 2,0,1,1)
        self._burst_interval_range_range = Range(1, 1000, .1, 100, 200)
        self._burst_interval_range_win = RangeWidget(self._burst_interval_range_range, self.set_burst_interval_range, 'Burst Interval', "counter_slider", float)
        self.top_grid_layout.addWidget(self._burst_interval_range_win, 2,1,1,1)
        self.blocks_tag_debug_0 = blocks.tag_debug(gr.sizeof_gr_complex*1, '', ""); self.blocks_tag_debug_0.set_display(True)
        self.analog_random_source_x_0 = blocks.vector_source_b(map(int, numpy.random.randint(0, 255, 100000)), True)
        self._alpha_range_range = Range(.1, 1, .05, .1, 200)
        self._alpha_range_win = RangeWidget(self._alpha_range_range, self.set_alpha_range, 'Alpha', "counter_slider", float)
        self.top_layout.addWidget(self._alpha_range_win)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.analog_random_source_x_0, 0), (self.digital_psk_mod_0, 0))
        self.connect((self.digital_psk_mod_0, 0), (self.blocks_probe_rate_0, 0))
        self.connect((self.digital_psk_mod_0, 0), (self.blocks_tag_debug_0, 0))
        self.connect((self.digital_psk_mod_0, 0), (self.sidekiq_sidekiq_tx_0, 0))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_variable_function_probe_0(self):
        return self.variable_function_probe_0

    def set_variable_function_probe_0(self, variable_function_probe_0):
        self.variable_function_probe_0 = variable_function_probe_0
        self.set_variable_qtgui_label_0(self._variable_qtgui_label_0_formatter(self.variable_function_probe_0))

    def get_sample_rate(self):
        return self.sample_rate

    def set_sample_rate(self, sample_rate):
        self.sample_rate = sample_rate
        self.set_bandwidth(self.sample_rate)
        self.sidekiq_sidekiq_tx_0.set_tx_sample_rate(self.sample_rate)
        self.set_rrc_filter_taps(firdes.root_raised_cosine(1, self.sample_rate, self.sample_rate/2.0, self.alpha_range, 128))

    def get_alpha_range(self):
        return self.alpha_range

    def set_alpha_range(self, alpha_range):
        self.alpha_range = alpha_range
        self.set_rrc_filter_taps(firdes.root_raised_cosine(1, self.sample_rate, self.sample_rate/2.0, self.alpha_range, 128))
        self.set_gaussian_filter_taps(firdes.gaussian(1.0, 2.0, self.alpha_range, 128))

    def get_vector_length(self):
        return self.vector_length

    def set_vector_length(self, vector_length):
        self.vector_length = vector_length

    def get_variable_qtgui_label_0(self):
        return self.variable_qtgui_label_0

    def set_variable_qtgui_label_0(self, variable_qtgui_label_0):
        self.variable_qtgui_label_0 = variable_qtgui_label_0
        Qt.QMetaObject.invokeMethod(self._variable_qtgui_label_0_label, "setText", Qt.Q_ARG("QString", eng_notation.num_to_str(self.variable_qtgui_label_0)))

    def get_source_min_output_buffer(self):
        return self.source_min_output_buffer

    def set_source_min_output_buffer(self, source_min_output_buffer):
        self.source_min_output_buffer = source_min_output_buffer

    def get_rrc_filter_taps(self):
        return self.rrc_filter_taps

    def set_rrc_filter_taps(self, rrc_filter_taps):
        self.rrc_filter_taps = rrc_filter_taps

    def get_max_sample_rate(self):
        return self.max_sample_rate

    def set_max_sample_rate(self, max_sample_rate):
        self.max_sample_rate = max_sample_rate

    def get_gaussian_filter_taps(self):
        return self.gaussian_filter_taps

    def set_gaussian_filter_taps(self, gaussian_filter_taps):
        self.gaussian_filter_taps = gaussian_filter_taps

    def get_gain(self):
        return self.gain

    def set_gain(self, gain):
        self.gain = gain
        self.sidekiq_sidekiq_tx_0.set_tx_attenuation(self.gain)

    def get_center_freq(self):
        return self.center_freq

    def set_center_freq(self, center_freq):
        self.center_freq = center_freq
        self.sidekiq_sidekiq_tx_0.set_tx_frequency(self.center_freq)

    def get_burst_length_range(self):
        return self.burst_length_range

    def set_burst_length_range(self, burst_length_range):
        self.burst_length_range = burst_length_range

    def get_burst_interval_range(self):
        return self.burst_interval_range

    def set_burst_interval_range(self, burst_interval_range):
        self.burst_interval_range = burst_interval_range

    def get_bandwidth(self):
        return self.bandwidth

    def set_bandwidth(self, bandwidth):
        self.bandwidth = bandwidth
        self.sidekiq_sidekiq_tx_0.set_tx_bandwidth(self.bandwidth)


def main(top_block_cls=top_block, options=None):

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
