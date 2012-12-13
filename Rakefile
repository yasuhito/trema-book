# -*- coding: utf-8 -*-

task :default => "ja:pdf"
task :pdf => "ja:pdf"
task :epub => "ja:epub"
task :mobi => "ja:mobi"
task :clean => "ja:clean"


def add_pdf_task lang, description
  desc description
  task :pdf => "clean:pdf" do
    cd lang do
      sh "review-pdfmaker trema.yaml"
    end
  end

  namespace :clean do
    task :pdf do
      sh "rm -f ./#{ lang }/trema.pdf"
    end
  end
  task :clean => "#{ lang }:clean:pdf"
end


def add_epub_task lang, description
  desc description
  task :epub => "clean:epub" do
    cd lang do
      sh "review-epubmaker trema.yaml"
    end
  end

  namespace :clean do
    task :epub do
      sh "rm -f ./#{ lang }/trema.epub"
    end
  end
  task :clean => "#{ lang }:clean:epub"
end


def add_mobi_task lang, description
  desc description
  task :mobi => [ :epub, "clean:mobi" ] do
    if `which kindleGen`.empty?
      raise "kindleGen is not installed!"
    end
    cd lang do
      sh "KindleGen trema.epub" rescue nil
    end
  end

  namespace :clean do
    task :mobi do
      sh "rm -f ./#{ lang }/trema.mobi"
    end
  end
  task :clean => "#{ lang }:clean:mobi"
end


namespace :ja do
  add_pdf_task "ja", "PDF を生成"
  add_epub_task "ja", "epub を生成"
  add_mobi_task "ja", "mobi を生成"
end


namespace :en do
  add_pdf_task "en", "Generates PDF"
  add_epub_task "en", "Generates epub"
  add_mobi_task "en", "Generates mobi"
end


### Local variables:
### mode: Ruby
### coding: utf-8
### indent-tabs-mode: nil
### End:
