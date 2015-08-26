require 'asciidoctor'

task :build do
  Asciidoctor.render_file('hello_trema.adoc',
                          in_place: true,
                          backend: 'html5')
end

task travis: :build

Dir.glob('tasks/*.rake').each { |each| import each }
