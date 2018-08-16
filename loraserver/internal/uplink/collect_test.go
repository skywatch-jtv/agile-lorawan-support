package uplink

import (
	"fmt"
	"sync"
	"testing"
	"time"

	"github.com/brocaar/loraserver/api/gw"
	"github.com/brocaar/loraserver/internal/common"
	"github.com/brocaar/loraserver/internal/config"
	"github.com/brocaar/loraserver/internal/models"
	"github.com/brocaar/loraserver/internal/test"
	"github.com/brocaar/lorawan"
	. "github.com/smartystreets/goconvey/convey"
)

func TestCollectAndCallOnce(t *testing.T) {
	conf := test.GetConfig()
	p := common.NewRedisPool(conf.RedisURL)
	config.C.Redis.Pool = p
	config.C.NetworkServer.DeduplicationDelay = time.Millisecond * 500

	Convey("Given a Redis connection pool", t, func() {
		test.MustFlushRedis(p)
		test.MustPrefillRedisPool(p, 3)

		Convey("Given a single LoRaWAN packet", func() {
			testTable := []struct {
				PHYPayload lorawan.PHYPayload
				Gateways   []lorawan.EUI64
				Count      int
			}{
				{
					lorawan.PHYPayload{
						MHDR: lorawan.MHDR{
							MType: lorawan.UnconfirmedDataUp,
							Major: lorawan.LoRaWANR1,
						},
						MIC:        [4]byte{1, 2, 3, 4},
						MACPayload: &lorawan.MACPayload{},
					},
					[]lorawan.EUI64{
						{1, 1, 1, 1, 1, 1, 1, 1},
					},
					1,
				}, {
					lorawan.PHYPayload{
						MHDR: lorawan.MHDR{
							MType: lorawan.UnconfirmedDataUp,
							Major: lorawan.LoRaWANR1,
						},
						MIC:        [4]byte{2, 2, 3, 4},
						MACPayload: &lorawan.MACPayload{},
					},
					[]lorawan.EUI64{
						{2, 1, 1, 1, 1, 1, 1, 1},
						{2, 2, 2, 2, 2, 2, 2, 2},
					},
					2,
				}, {
					lorawan.PHYPayload{
						MHDR: lorawan.MHDR{
							MType: lorawan.UnconfirmedDataUp,
							Major: lorawan.LoRaWANR1,
						},
						MIC:        [4]byte{3, 2, 3, 4},
						MACPayload: &lorawan.MACPayload{},
					},
					[]lorawan.EUI64{
						{3, 1, 1, 1, 1, 1, 1, 1},
						{3, 2, 2, 2, 2, 2, 2, 2},
						{3, 2, 2, 2, 2, 2, 2, 2},
					},
					2,
				},
			}

			for i, test := range testTable {
				Convey(fmt.Sprintf("When running test %d, then %d items in the RXInfoSet are expected", i, test.Count), func() {
					var received int
					var called int

					dr0, err := config.C.NetworkServer.Band.Band.GetDataRate(0)
					So(err, ShouldBeNil)

					cb := func(packet models.RXPacket) error {
						called = called + 1
						received = len(packet.RXInfoSet)
						return nil
					}

					var wg sync.WaitGroup
					for _, g := range test.Gateways {
						wg.Add(1)
						packet := gw.RXPacket{
							RXInfo: gw.RXInfo{
								MAC:      g,
								DataRate: dr0,
							},
							PHYPayload: test.PHYPayload,
						}
						go func() {
							err := collectAndCallOnce(p, packet, cb)
							if err != nil {
								t.Error(err)
							}
							wg.Done()
						}()
					}
					wg.Wait()

					So(called, ShouldEqual, 1)
					So(received, ShouldEqual, test.Count)
				})
			}
		})

	})
}
