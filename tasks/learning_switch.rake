VENDOR_LEARNING_SWITCH_DIR =
  File.expand_path('../../vendor/learning_switch', __FILE__)

desc 'Clone learning_switch source code into vendor'
task vendor_learning_switch: VENDOR_LEARNING_SWITCH_DIR

directory 'vendor'

directory VENDOR_LEARNING_SWITCH_DIR => 'vendor' do
  cd 'vendor' do
    sh 'git clone https://github.com/trema/learning_switch.git'
  end
end
