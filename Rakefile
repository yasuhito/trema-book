# -*- coding: utf-8 -*-

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
  # config.stylesheets.heading_font        = 'PT Sans', sans-serif'
  # config.stylesheets.font                = 'PT Serif', serif'
  # config.add_font('PT Sans', file: 'fonts/PT_Sans-Web-Regular.ttf')
  # config.add_font(
  #   'PT Sans',
  #   weight: 'bold',
  #   file: 'fonts/PT_Sans-Web-Bold.ttf')
  # config.add_font(
  #   'PT Sans',
  #   style:  'italic',
  #   file: 'fonts/PT_Sans-Web-Italic.ttf')
  # config.add_font(
  #   'PT Sans',
  #   weight: 'bold',
  #   style:  'italic',
  #   file: 'fonts/PT_Sans-Web-BoldItalic.ttf')
  # config.add_font('PT Serif', file: 'fonts/PT_Serif-Web-Regular.ttf')
  # config.add_font(
  #   'PT Serif',
  #   weight: 'bold',
  #   file: 'fonts/PT_Serif-Web-Bold.ttf')
  # config.add_font(
  #   'PT Serif',
  #   style:  'italic',
  #   file: 'fonts/PT_Serif-Web-Italic.ttf')
  # config.add_font(
  #   'PT Serif',
  #   weight: 'bold',
  #   style:  'italic',
  #   file: 'fonts/PT_Serif-Web-BoldItalic.ttf')
  # config.add_font('Source Code Pro', file: 'fonts/SourceCodePro-Regular.otf')
  # config.add_font(
  #   'Source Code Pro',
  #   weight: 'bold',
  #   file: 'fonts/SourceCodePro-Bold.otf')
end

# task :default => "ja:pdf"
# task :pdf => "ja:pdf"
# task :epub => "ja:epub"
# task :mobi => "ja:mobi"
# task :clean => "ja:clean"


# def add_pdf_task lang, description
#   desc description
#   task :pdf => "clean:pdf" do
#     cd lang do
#       sh "review-pdfmaker trema.yaml"
#     end
#   end

#   namespace :clean do
#     task :pdf do
#       sh "rm -f ./#{ lang }/trema.pdf"
#     end
#   end
#   task :clean => "#{ lang }:clean:pdf"
# end


# def add_epub_task lang, description
#   desc description
#   task :epub => "clean:epub" do
#     cd lang do
#       sh "review-epubmaker trema.yaml"
#     end
#   end

#   namespace :clean do
#     task :epub do
#       sh "rm -f ./#{ lang }/trema.epub"
#     end
#   end
#   task :clean => "#{ lang }:clean:epub"
# end


# def add_mobi_task lang, description
#   desc description
#   task :mobi => [ :epub, "clean:mobi" ] do
#     if `which kindleGen`.empty?
#       raise "kindleGen is not installed!"
#     end
#     cd lang do
#       sh "KindleGen trema.epub" rescue nil
#     end
#   end

#   namespace :clean do
#     task :mobi do
#       sh "rm -f ./#{ lang }/trema.mobi"
#     end
#   end
#   task :clean => "#{ lang }:clean:mobi"
# end


# namespace :ja do
#   add_pdf_task "ja", "PDF を生成"
#   add_epub_task "ja", "epub を生成"
#   add_mobi_task "ja", "mobi を生成"
# end


# namespace :en do
#   add_pdf_task "en", "Generates PDF"
#   add_epub_task "en", "Generates epub"
#   add_mobi_task "en", "Generates mobi"
# end
