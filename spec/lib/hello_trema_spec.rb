require 'trema'
require 'hello_trema'

describe HelloTrema do
  Given(:logger) { spy('logger') }
  Given(:hello_trema) do
    HelloTrema.new.tap do |controller|
      allow(controller).to receive(:logger).and_return(logger)
    end
  end

  describe '#start' do
    When { hello_trema.start(args) }

    context 'with []' do
      Given(:args) { [] }
      Then do
        expect(logger).to(have_received(:info).with('Trema started.'))
      end
    end
  end

  describe '#switch_ready' do
    When { hello_trema.switch_ready(dpid) }

    context 'with 0xabc' do
      Given(:dpid) { 0xabc }
      Then { expect(logger).to have_received(:info).with('Hello 0xabc!') }
    end
  end
end
