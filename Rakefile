# -*- coding: utf-8 -*-

task :default => :pdf


namespace :clean do
  task :pdf do
    sh "rm -f trema.pdf"
  end


  task :epub do
    sh "rm -f trema.epub"
  end


  task :mobi do
    sh "rm -f trema.mobi"
  end
end

task :clean => [ "clean:pdf", "clean:epub", "clean:mobi" ]


desc "PDF を生成"
task :pdf => "clean:pdf" do
  sh "review-pdfmaker trema.yaml"
end


desc "epub を生成"
task :epub => "clean:epub" do
  sh "review-epubmaker trema.yaml"
end


desc "mobi を生成"
task :mobi => [ :epub, "clean:mobi" ] do
  if `which kindleGen`.empty?
    raise "kindleGen is not installed!"
  end
  sh "KindleGen trema.epub" rescue nil
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
