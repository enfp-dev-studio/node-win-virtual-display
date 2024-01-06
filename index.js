"use strict";
const addon = require("bindings")("virtual_display.node");

function VirtualDisplay() {
  var _addonInstance = new addon.VirtualDisplay();

  this.createVirtualDisplay = function () {
    // 네이티브 메서드 호출
    const ret = _addonInstance.createVirtualDisplay();

    // 결과 로깅
    console.log(ret);

    // 반환 객체 구성
    // return {
    //   id: ret.id,
    //   width: ret.width,
    //   height: ret.height,
    // };
  };

  this.destroyVirtualDisplay = function () {
    // 네이티브 메서드 호출
    return _addonInstance.destroyVirtualDisplay();
  };
}

module.exports = VirtualDisplay;