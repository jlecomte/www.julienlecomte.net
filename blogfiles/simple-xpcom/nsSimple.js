/****************************************************************************
 *
 * nsSimple.js
 *
 * Copyright (C) 2007 - Julien Lecomte
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 ****************************************************************************/


/******************************************************************************
 * Constants
 ******************************************************************************/
const CLASS_ID = Components.ID("{D486A7F3-17E7-4616-9D17-9C2C6692BF11}");
const CLASS_NAME = "Simple XPCOM Component";
const CONTRACT_ID = "@mozilla.org/js_simple_component;1";


/******************************************************************************
 * nsSimple class definition
 ******************************************************************************/

function nsSimple() {}

nsSimple.prototype = {

    // nsISimple

    version: "1.0",

    doSomething: function(value) {
        return value;
    },

    // nsIClassInfo

    getInterfaces: function(count) {
        var ifaces = new Array();
        ifaces.push(Components.interfaces.nsISimple);
        ifaces.push(Components.interfaces.nsIClassInfo);
        ifaces.push(Components.interfaces.nsISupports);
        count.value = ifaces.length;
        return ifaces;
    },

    getHelperForLanguage: function(language) {
        return null;
    },

    contractID: CONTRACT_ID,
    classID: CLASS_ID,
    classDescription: CLASS_NAME,
    flags: Components.interfaces.nsIClassInfo.DOM_OBJECT,
    implementationLanguage: Components.interfaces.nsIProgrammingLanguage.JAVASCRIPT,

    // nsISupports

    QueryInterface: function(iid) {
        if (!iid.equals(Components.interfaces.nsISimple) &&
            !iid.equals(Components.interfaces.nsIClassInfo) &&
            !iid.equals(Components.interfaces.nsISupports)) {
            throw Components.results.NS_ERROR_NO_INTERFACE;
        }
        return this;
    }
};


/******************************************************************************
 * XPCOM objects and functions for construction and registration
 ******************************************************************************/

var nsSimpleModule = {

    firstTime: true,

    registerSelf: function(compMgr, fileSpec, location, type) {

        if (this.firstTime) {
            this.firstTime = false;
            throw Components.results.NS_ERROR_FACTORY_REGISTER_AGAIN;
        }

        compMgr = compMgr.QueryInterface(Components.interfaces.nsIComponentRegistrar);
        compMgr.registerFactoryLocation(CLASS_ID, CLASS_NAME, CONTRACT_ID, fileSpec, location, type);

        // Register as global javascript property
        var catMgr = Components.classes["@mozilla.org/categorymanager;1"].getService(Components.interfaces.nsICategoryManager);
        catMgr.addCategoryEntry("JavaScript global property", "simpleObj", CONTRACT_ID, true, true);
    },

    unregisterSelf: function(compMgr, location, type) {
        compMgr = compMgr.QueryInterface(Components.interfaces.nsIComponentRegistrar);
        compMgr.unregisterFactoryLocation(CLASS_ID, location);

        // Unregister as global javascript property
        var catMgr = Components.classes["@mozilla.org/categorymanager;1"].getService(Components.interfaces.nsICategoryManager);
        catMgr.deleteCategoryEntry("JavaScript global property", "simpleObj", CONTRACT_ID, true);
    },

    getClassObject: function(compMgr, cid, iid) {

        if (!iid.equals(Components.interfaces.nsIFactory))
            throw Components.results.NS_ERROR_NOT_IMPLEMENTED;

        if (cid.equals(CLASS_ID))
            return nsSimpleFactory;

        throw Components.results.NS_ERROR_NO_INTERFACE;
    },

    canUnload: function (compMgr) {
        return true;
    }
};

var nsSimpleFactory = {

    createInstance: function(outer, iid) {
        if (outer != null)
            throw Components.results.NS_ERROR_NO_AGGREGATION;
        return (new nsSimple()).QueryInterface(iid);
    }
};

function NSGetModule(compMgr, fileSpec) {
    return nsSimpleModule;
}
