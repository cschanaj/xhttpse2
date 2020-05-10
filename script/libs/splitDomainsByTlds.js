"use strict";

/**
 *
 * @param {string[]} domains An array of domains to be sorted
 */
function splitDomainsByTlds(domains) {
  let retval = [domains];
  for (let tldCounts = 1; tldCounts < 2; ++tldCounts) {
    let tlds = domains.map((domain) =>
      domain.split(".").reverse().slice(0, tldCounts).reverse().join(".")
    );

    // remove possible duplicates
    tlds = Array.from(new Set(tlds));

    if (tlds.length > 1) {
      retval = tlds.map((tld) =>
        domains.filter((domain) => domain.endsWith(`.${tld}`))
      );
      break;
    }
  }
  return retval;
}

module.exports = splitDomainsByTlds;
