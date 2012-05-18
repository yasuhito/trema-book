= Trema ライブラリ

この章は、Trema が提供するクラスおよびモジュールのリファレンスです。こ
れらのクラスは、すべての Trema アプリケーションで直接利用できます。
require は必要ありません。

== class PacketIn < Object

@<tt>{Controller#packet_in} に渡されるパケットインしたイーサネットフレー
ムの情報を含むオブジェクトです。


#@# メソッドを適切にグループ分けしたい
#@# ちょっとしたサンプルコードをここに


=== インスタンスメソッド

==== arp?
      * @<i>{packet_in}.arp? → @<tt>{true} or @<tt>{false}

     @<i>{packet_in} が ARP パケットのとき @<tt>{true} を返します。


==== buffer_id
      * @<i>{packet_in}.buffer_id → @<i>{int}

      この値はフレーム全体が @<i>{packet_in} のデータフィールド
      @<tt>{PacketIn#data} に含まれているか (パケットはスイッチにバッファ
      されていない)、一部がデータフィールドに含まれている (パケットがス
      イッチにバッファされている)ことを示す。

#@# Nick さんの API コメント、出来がけっこうひどいかも。


==== buffered?
      * @<i>{packet_in}.buffered? → @<tt>{true} or @<tt>{false}

      パケットがスイッチにバッファされているとき @<tt>{true} を返しま
      す。


==== data
      * @<i>{packet_in}.data → @<i>{string}

      パケットインとして入ってきたイーサネットフレーム全体のデータを返
      します。


==== datapath_id
      * @<i>{packet_in}.datapath_id → @<i>{int}

      パケットインを出したスイッチの ID を返します。


==== eth_type
     * @<i>{packet_in}.eth_type → @<i>{int}

     イーサネットタイプの値を返します。


==== icmpv4?
      * @<i>{packet_in}.icmpv4? → @<tt>{true} or @<tt>{false}

      パケットが ICMPv4 のパケットであるとき @<tt>{true} を返します。


==== igmp?
      * @<i>{packet_in}.igmp? → @<tt>{true} or @<tt>{false}

      パケットが IGMP のパケットであるとき @<tt>{true} を返します。


==== in_port
     * @<i>{packet_in}.in_port → @<i>{int}

     フレームを受けたスイッチのポート番号を返します。


==== ipv4?
      * @<i>{packet_in}.ipv4? → @<tt>{true} or @<tt>{false}

      パケットが IPv4 のパケットであるとき @<tt>{true} を返します。


==== macda
      * @<i>{packet_in}.macda → @<i>{mac}

      フレームの MAC destination アドレスを返します。


==== macsa
      * @<i>{packet_in}.macsa → @<i>{mac}

      フレームの MAC source アドレス を返します。


==== reason
      * @<i>{packet_in}.reason → @<i>{int}

      パケットインが起こった理由を返します。


==== tcp?
      * @<i>{packet_in}.tcp? → @<tt>{true} or @<tt>{false}

      パケットが TCP のパケットであるとき @<tt>{true} を返します。


==== total_len
      * @<i>{packet_in}.total_len → @<i>{int}

      フレームの全長を返します。


==== transaction_id
      * @<i>{packet_in}.transaction_id → @<i>{int}

      トランザクション ID を返します。


==== udp?
      * @<i>{packet_in}.udp? → @<tt>{true} or @<tt>{false}

      パケットが UDP のパケットであるとき @<tt>{true} を返します。


==== vtag?
      * @<i>{packet_in}.vtag? → @<tt>{true} or @<tt>{false}

      パケットに VLAN タグがついているとき @<tt>{true} を返します。


== class ActionOutput < Object


=== インスタンスメソッド


==== append


==== initialize


==== inspect


==== max_len


==== port


== class ExactMatch < Object


=== インスタンスメソッド


==== from


== class FlowRemoved < Object


=== インスタンスメソッド


==== byte_count


==== cookie


==== datapath_id
     * @<i>{packet_in}.datapath_id → @<i>{int}

      フローリムーブドを出したスイッチの ID を返します。


==== duration_nsec


==== duration_sec


==== idle_timeout


==== initialize


==== match


==== packet_count


==== priority


==== reason


==== transaction_id


== class Match < Object


=== インスタンスメソッド


==== compare


==== dl_dst


==== dl_src


==== dl_type


==== dl_vlan


==== dl_vlan_pcp


==== in_port


==== initialize


==== nw_dst


==== nw_proto


==== nw_src


==== nw_tos


==== replace


==== to_s


==== tp_dst


==== tp_src


==== wildcards
     * @<i>{match}.wildcards → @<i>{Number}

     ワイルドカードを指定します。