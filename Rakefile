# -*- coding: utf-8 -*-

task :default => "ja:pdf"
task :pdf => "ja:pdf"
task :epub => "ja:epub"
task :mobi => "ja:mobi"
task :clean => "ja:clean"


namespace :ja do
  namespace :clean do
    task :pdf do
      sh "rm -f ./ja/trema.pdf"
    end


    task :epub do
      sh "rm -f ./ja/trema.epub"
    end


    task :mobi do
      sh "rm -f ./ja/trema.mobi"
    end
  end

  task :clean => [ "ja:clean:pdf", "ja:clean:epub", "ja:clean:mobi" ]


  desc "PDF を生成"
  task :pdf => "clean:pdf" do
    cd "ja" do
      sh "review-pdfmaker trema.yaml"
    end
  end


  desc "epub を生成"
  task :epub => "clean:epub" do
    cd "ja" do
      sh "review-epubmaker trema.yaml"
    end
  end


  desc "mobi を生成"
  task :mobi => [ :epub, "clean:mobi" ] do
    if `which kindleGen`.empty?
      raise "kindleGen is not installed!"
    end
    cd "ja" do
      sh "KindleGen trema.epub" rescue nil
    end
  end


  desc "章ごとのページ数などを表示"
  task :vol do
    cd "ja" do
      sh "review-vol trema.yaml"
    end
  end
end


### Local variables:
### mode: Ruby
### coding: utf-8
### indent-tabs-mode: nil
### End:
