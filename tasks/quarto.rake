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
    'openflow_framework_trema.org',
    'learning_switch.org',
    'routing_switch.org',
    'sliceable_switch.org'
  ]
  config.stylesheets.cover_color  = '#fff4cd'

  config.stylesheets.heading_font = '"Hiragino Kaku Gothic Pro", sans-serif'
  config.stylesheets.font         = '"Hiragino Mincho Pro", serif'
end
