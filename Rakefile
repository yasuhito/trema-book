require "rake/clean"

CLEAN << "trema.pdf"
CLEAN << "trema.epub"


task :pdf do
  sh "review-pdfmaker trema.yaml"
end


task :epub do
  sh "review-epubmaker trema.yaml"
end


task :vol do
  sh "review-vol trema.yaml"
end


### Local variables:
### mode: Ruby
### coding: utf-8
### indent-tabs-mode: nil
### End:
