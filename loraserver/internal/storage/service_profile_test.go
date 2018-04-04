package storage

import (
	"testing"
	"time"

	"github.com/brocaar/loraserver/internal/common"
	"github.com/brocaar/loraserver/internal/config"
	"github.com/brocaar/loraserver/internal/test"
	"github.com/brocaar/lorawan/backend"
	"github.com/pkg/errors"
	. "github.com/smartystreets/goconvey/convey"
)

func TestServiceProfile(t *testing.T) {
	conf := test.GetConfig()
	db, err := common.OpenDatabase(conf.PostgresDSN)
	if err != nil {
		t.Fatal(err)
	}
	config.C.PostgreSQL.DB = db
	config.C.Redis.Pool = common.NewRedisPool(conf.RedisURL)

	Convey("Given a clean database", t, func() {
		test.MustResetDB(config.C.PostgreSQL.DB)
		test.MustFlushRedis(config.C.Redis.Pool)

		Convey("When creating a service-profile", func() {
			sp := ServiceProfile{
				ServiceProfile: backend.ServiceProfile{
					ULRate:                 1,
					ULBucketSize:           2,
					ULRatePolicy:           backend.Drop,
					DLRate:                 3,
					DLBucketSize:           4,
					DLRatePolicy:           backend.Mark,
					AddGWMetadata:          true,
					DevStatusReqFreq:       5,
					ReportDevStatusBattery: true,
					ReportDevStatusMargin:  true,
					DRMin:          6,
					DRMax:          7,
					ChannelMask:    backend.HEXBytes{1, 2, 3},
					PRAllowed:      true,
					HRAllowed:      true,
					RAAllowed:      true,
					NwkGeoLoc:      true,
					TargetPER:      1,
					MinGWDiversity: 8,
				},
			}

			So(CreateServiceProfile(db, &sp), ShouldBeNil)
			sp.CreatedAt = sp.CreatedAt.UTC().Truncate(time.Millisecond)
			sp.UpdatedAt = sp.UpdatedAt.UTC().Truncate(time.Millisecond)

			Convey("Then GetServiceProfile returns the expected service-profile", func() {
				spGet, err := GetServiceProfile(db, sp.ServiceProfile.ServiceProfileID)
				So(err, ShouldBeNil)

				spGet.CreatedAt = spGet.CreatedAt.UTC().Truncate(time.Millisecond)
				spGet.UpdatedAt = spGet.UpdatedAt.UTC().Truncate(time.Millisecond)
				So(spGet, ShouldResemble, sp)
			})

			Convey("Then UpdateServiceProfile updates the service-profile", func() {
				sp.ServiceProfile = backend.ServiceProfile{
					ServiceProfileID:       sp.ServiceProfile.ServiceProfileID,
					ULRate:                 2,
					ULBucketSize:           3,
					ULRatePolicy:           backend.Mark,
					DLRate:                 4,
					DLBucketSize:           5,
					DLRatePolicy:           backend.Drop,
					AddGWMetadata:          false,
					DevStatusReqFreq:       6,
					ReportDevStatusBattery: false,
					ReportDevStatusMargin:  false,
					DRMin:          7,
					DRMax:          8,
					ChannelMask:    backend.HEXBytes{3, 2, 1},
					PRAllowed:      false,
					HRAllowed:      false,
					RAAllowed:      false,
					NwkGeoLoc:      false,
					TargetPER:      2,
					MinGWDiversity: 9,
				}
				So(UpdateServiceProfile(db, &sp), ShouldBeNil)
				sp.UpdatedAt = sp.UpdatedAt.UTC().Truncate(time.Millisecond)

				spGet, err := GetServiceProfile(db, sp.ServiceProfile.ServiceProfileID)
				So(err, ShouldBeNil)

				spGet.CreatedAt = spGet.CreatedAt.UTC().Truncate(time.Millisecond)
				spGet.UpdatedAt = spGet.UpdatedAt.UTC().Truncate(time.Millisecond)
				So(spGet, ShouldResemble, sp)
			})

			Convey("Then DeleteServiceProfile deletes the service-profile", func() {
				So(DeleteServiceProfile(db, sp.ServiceProfile.ServiceProfileID), ShouldBeNil)
				So(DeleteServiceProfile(db, sp.ServiceProfile.ServiceProfileID), ShouldEqual, ErrDoesNotExist)
			})

			Convey("Then GetAndCacheServiceProfile reads the service-profile from db and puts it in cache", func() {
				spGet, err := GetAndCacheServiceProfile(config.C.PostgreSQL.DB, config.C.Redis.Pool, sp.ServiceProfile.ServiceProfileID)
				So(err, ShouldBeNil)
				So(spGet.ServiceProfile.ServiceProfileID, ShouldEqual, sp.ServiceProfile.ServiceProfileID)

				Convey("Then GetServiceProfileCache returns the service-profile", func() {
					spGet, err := GetServiceProfileCache(config.C.Redis.Pool, sp.ServiceProfile.ServiceProfileID)
					So(err, ShouldBeNil)
					So(spGet.ServiceProfile.ServiceProfileID, ShouldEqual, sp.ServiceProfile.ServiceProfileID)
				})

				Convey("Then FlushServiceProfileCache removes the service-profile from cache", func() {
					err := FlushServiceProfileCache(config.C.Redis.Pool, sp.ServiceProfile.ServiceProfileID)
					So(err, ShouldBeNil)

					_, err = GetServiceProfileCache(config.C.Redis.Pool, sp.ServiceProfile.ServiceProfileID)
					So(err, ShouldNotBeNil)
					So(errors.Cause(err), ShouldEqual, ErrDoesNotExist)
				})
			})
		})
	})
}
