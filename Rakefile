# -*- coding: utf-8 -*-

task :default => :pdf


namespace :clean do
  task :pdf do
    sh "rm -f trema.pdf"
  end


  task :epub do
    sh "rm -f trema.epub"
  end
end

task :clean => [ "clean:pdf", "clean:epub" ]


desc "PDF を生成"
task :pdf => "clean:pdf" do
  sh "review-pdfmaker trema.yaml"
end


desc "epub を生成"
task :epub => "clean:epub" do
  sh "review-epubmaker trema.yaml"
end


desc "章ごとのページ数などを表示"
task :vol do
  sh "review-vol trema.yaml"
end


### Local variables:
### mode: Ruby
### coding: utf-8
### indent-tabs-mode: nil
### End:
