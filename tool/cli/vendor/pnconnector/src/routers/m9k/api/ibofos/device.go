package ibofos

import (
	"pnconnector/src/routers/m9k/model"
)

func ScanDevice(xrId string, param interface{}) (model.Request, model.Response, error) {
	return deviceSender(xrId, param, "SCANDEVICE")
}

func ListDevice(xrId string, param interface{}) (model.Request, model.Response, error) {
	return deviceSender(xrId, param, "LISTDEVICE")
}

func GetSMART(xrId string, param interface{}) (model.Request, model.Response, error) {
	return deviceSender(xrId, param, "SMART")
}

func deviceSender(xrId string, param interface{}, command string) (model.Request, model.Response, error) {
	return Requester{xrId, param, model.DeviceParam{}}.Send(command)
}
