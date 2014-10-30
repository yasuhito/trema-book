# -*- coding: utf-8 -*-

Dir.glob('tasks/*.rake').each { |each| import each }

require 'quarto/tasks'
require 'quarto'

Quarto.configure do |config|
  config.author              = 'Yasuhito Takamiya'
  config.title               = 'Trema Book'

  config.use :git
  config.use :orgmode       # if you want to use org-mode
  config.use :prince
  config.use :pandoc_epub
  config.use :epubcheck
  config.use :kindlegen
  config.use :bundle
  config.source_files                    = ['index.org']
  # config.bitmap_cover_image              = 'images/cover-large.png'
  # config.vector_cover_image              = 'images/cover.svg'
  config.stylesheets.cover_color         = '#fff4cd'

  config.stylesheets.heading_font = '"Hiragino Kaku Gothic Pro", sans-serif'
  config.stylesheets.font         = '"Hiragino Mincho Pro", serif'

  # config.add_font('Source Code Pro', file: 'fonts/SourceCodePro-Regular.otf')
  # config.add_font(
  #   'Source Code Pro',
  #   weight: 'bold',
  #   file: 'fonts/SourceCodePro-Bold.otf')
end
