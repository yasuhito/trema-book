require 'rake/clean'
require_relative '../lib/env'

desc 'Build cbench executable'
task 'vendor:cbench' => CBENCH
file CBENCH => OPENFLOW_H do
  sh "tar xzf #{VENDOR_OFLOPS}.tar.gz -C #{VENDOR}"
  cd VENDOR_OFLOPS do
    sh "./configure --with-openflow-src-dir=#{VENDOR_OPENFLOW}"
    sh 'make'
  end
end

CLEAN << VENDOR_OFLOPS

file OPENFLOW_H do
  sh "tar xzf #{VENDOR_OPENFLOW}.tar.gz -C #{VENDOR}"
end

CLEAN << VENDOR_OPENFLOW
