require 'asciidoctor'

task :build do
  Asciidoctor.render_file('book.adoc',
                          in_place: true,
                          backend: 'html5')
end

task default: :build
task travis: :build

Dir.glob('tasks/*.rake').each { |each| import each }
