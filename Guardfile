guard 'rake', task: 'deliverables' do
  watch(/.+\.org$/)
end

guard :rubocop do
  watch(/.+\.rb$/)
  watch(/(?:.+\/)?\.rubocop\.yml$/) { |m| File.dirname(m[0]) }
end
