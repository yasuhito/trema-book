require 'trema'
require 'hello_trema'

describe HelloTrema do
  Given(:hello_trema) { HelloTrema.new }

  describe '#switch_ready' do
    When(:output) { hello_trema.switch_ready(dpid) }

    context 'with 0xabc' do
      When(:dpid) { 0xabc }
      Then { output == 'Hello 0xabc!' }
    end
  end
end
