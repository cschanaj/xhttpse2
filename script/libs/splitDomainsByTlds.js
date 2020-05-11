"use strict";

/**
 *
 * @param {string[]} domains An array of domains to be sorted
 */
function splitDomainsByTlds(domains) {
  let retval = [domains];
  for (let tldCounts = 1; tldCounts < 3; ++tldCounts) {
    let tlds = domains.map((domain) =>
      domain.split(".").reverse().slice(0, tldCounts).reverse().join(".")
    );

    // remove possible duplicates
    tlds = Array.from(new Set(tlds));
    let tmp = tlds
      .map((tld) =>
        domains.filter((domain) => domain.endsWith(`.${tld}`) || domain === tld)
      )
      .filter((arr) => arr.length > 0);

    if (tmp.length > 1) {
      retval = tmp;
      break;
    }
  }
  return retval;
}

module.exports = splitDomainsByTlds;
