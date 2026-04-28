# Publishing the LPL Extension to the VS Code Marketplace

## Step 1 — Create a Microsoft account
Go to [https://account.microsoft.com](https://account.microsoft.com) and sign in or create a free account.

---

## Step 2 — Create a publisher on the Marketplace
1. Go to [https://marketplace.visualstudio.com/manage](https://marketplace.visualstudio.com/manage)
2. Sign in with your Microsoft account
3. Click **Create publisher**
4. Choose a unique **publisher ID** (e.g. `yourname`) — this is permanent
5. Fill in Display Name and click **Create**

---

## Step 3 — Create a Personal Access Token (PAT)
1. Go to [https://dev.azure.com](https://dev.azure.com) and sign in
2. Click your profile icon → **Personal access tokens** → **New Token**
3. Set:
   - **Organization**: `All accessible organizations`
   - **Scopes**: Custom defined → *Marketplace* → check **Manage**
4. Click **Create** and copy the token immediately — you cannot see it again

---

## Step 4 — Install `vsce`
```bash
npm install -g @vscode/vsce
```

---

## Step 5 — Update `package.json` with required fields
```json
{
  "publisher": "YOUR-PUBLISHER-ID",
  "license": "MIT",
  "repository": {
    "type": "git",
    "url": "https://github.com/YOUR-USERNAME/lpl-extension"
  },
  "icon": "icon.png"
}
```

---

## Step 6 — Add a `LICENSE` file
Create a plain text `LICENSE` file in the root with your chosen license text (e.g. MIT).

---

## Step 7 — Add an icon (optional but recommended)
Place a `128×128` PNG called `icon.png` in the extension root.

---

## Step 8 — Package and test locally
```bash
vsce package
code --install-extension lpl-language-0.1.0.vsix
```
Open any `.lpl` file and verify syntax highlighting works.

---

## Step 9 — Login with `vsce`
```bash
vsce login YOUR-PUBLISHER-ID
```
Paste the PAT from Step 3 when prompted.

---

## Step 10 — Publish
```bash
vsce publish
```
Live at: `https://marketplace.visualstudio.com/items?itemName=YOUR-PUBLISHER-ID.lpl-language`

---

## Step 11 — Update later
```bash
vsce publish patch   # 0.1.0 → 0.1.1
vsce publish minor   # 0.1.0 → 0.2.0
vsce publish major   # 0.1.0 → 1.0.0
```

---

## Checklist before publishing
- [ ] `publisher` in `package.json` matches your publisher ID
- [ ] `description` is filled in
- [ ] `LICENSE` file exists
- [ ] Local `.vsix` test passes