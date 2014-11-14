VENDOR_HELLO_TREMA_DIR =
  File.expand_path('../../vendor/hello_trema', __FILE__)

desc 'Clone hello_trema source code into vendor'
task vendor_hello_trema: VENDOR_HELLO_TREMA_DIR

directory 'vendor'

directory VENDOR_HELLO_TREMA_DIR => 'vendor' do
  cd 'vendor' do
    sh 'git clone https://github.com/trema/hello_trema.git'
  end
end
