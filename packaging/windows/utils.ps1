function Remove-SafeItem {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path
    )
    
    try {
        $items = Get-ChildItem -Path $Path -Force -ErrorAction SilentlyContinue
        
        if ($items) {
            foreach ($item in $items) {
                if ($item.PSIsContainer) {
                    Remove-Item -Path $item.FullName -Recurse -Force -Verbose
                    Write-Host "Folder '$($item.FullName)' deleted."
                }
                else {
                    Remove-Item -Path $item.FullName -Force -Verbose
                    Write-Host "File '$($item.FullName)' deleted."
                }
            }
        }
        else {
            Write-Host "No items matched the path '$Path'."
        }
    }
    catch {
        Write-Error "An error occurred while deleting items: $_"
    }
}
