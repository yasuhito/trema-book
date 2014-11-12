require 'quarto/tasks'
require 'quarto'

Quarto.configure do |config|
  config.metadata = true

  config.author = 'Yasuhito Takamiya'
  config.title = 'Trema Book'
  config.language = 'ja'

  config.use :bundle
  config.use :git
  config.use :orgmode
  config.use :prince

  config.source_files = [
    'foreword.markdown',
    'ch1_whats_openflow.org',
    'ch2_openflow_usecases.org',
    'ch3_openflow_spec.org',
    'ch4_openflow_frameworks.org',
    'ch5_openflow_framework_trema.org',
    'ch8_learning_switch.org',
    'ch14_routing_switch.org',
    'ch15_sliceable_switch.org'
  ]
  config.stylesheets.cover_color  = '#fff4cd'

  config.stylesheets.heading_font = '"Hiragino Kaku Gothic Pro", sans-serif'
  config.stylesheets.font         = '"Hiragino Mincho Pro", serif'
end
