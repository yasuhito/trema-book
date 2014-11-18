# coding: utf-8
require 'quarto/tasks'
require 'quarto'

Quarto.configure do |config|
  config.metadata = true

  config.author = 'Yasuhito Takamiya'
  config.title = 'OpenFlow実践入門'
  config.language = 'ja'

  config.use :bundle
  config.use :git
  config.use :orgmode
  config.use :prince

  config.source_files = [
    'foreword.markdown',
    'foreword_edition1.markdown',
    'ch1_whats_openflow.org',
    'ch2_openflow_usecases.org',
    'ch3_openflow_spec.org',
    'ch4_openflow_frameworks.org',
    'ch5_openflow_framework_trema.org',
    'ch6_switch_monitoring_tool.org',
    'ch7_patch_panel.org',
    'ch8_learning_switch.org',
    'ch9_traffic_monitor.org',
    'ch10_router_part1.org',
    'ch11_router_part2.org',
    'ch14_routing_switch.org',
    'ch15_sliceable_switch.org'
  ]
  config.stylesheets.cover_color  = '#fff4cd'

  config.stylesheets.heading_font = '"Hiragino Kaku Gothic Pro", sans-serif'
  config.stylesheets.font         = '"Hiragino Mincho Pro", serif'
end
