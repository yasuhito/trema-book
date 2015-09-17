require 'trema'
require 'switch_monitor'

describe SwitchMonitor do
  Given(:logger) { spy('logger') }
  Given(:switch_monitor) do
    SwitchMonitor.new.tap do |controller|
      allow(controller).to receive(:logger).and_return(logger)
    end
  end

  describe '#start' do
    When { switch_monitor.start(args) }

    context 'with []' do
      Given(:args) { [] }
      Then do
        expect(logger).to(have_received(:info).with('SwitchMonitor started.'))
      end

      describe '#switch_ready' do
        When { switch_monitor.switch_ready(dpid1) }

        context 'with 0xabc' do
          Given(:dpid1) { 0xabc }
          Then do
            expect(logger).to(have_received(:info).
                              with('0xabc is up (All = 0xabc)'))
          end

          describe '#switch_ready' do
            When { switch_monitor.switch_ready(dpid2) }

            context 'with 0xdef' do
              Given(:dpid2) { 0xdef }
              Then do
                expect(logger).to(have_received(:info).
                                  with('0xdef is up (All = 0xabc, 0xdef)'))
              end

              describe '#switch_disconnected' do
                When { switch_monitor.switch_disconnected(dpid3) }

                context 'with 0xabc' do
                  Given(:dpid3) { 0xabc }
                  Then do
                    expect(logger).to(have_received(:info).
                                      with('0xabc is down (All = 0xdef)'))
                  end
                end
              end
            end
          end
        end
      end
    end
  end
end
